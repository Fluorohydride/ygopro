--魂を吸う竹光
function c51670553.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c51670553.target)
	e1:SetOperation(c51670553.operation)
	c:RegisterEffect(e1)
	--skip draw
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(51670553,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_DAMAGE)
	e2:SetCondition(c51670553.skipcon)
	e2:SetOperation(c51670553.skipop)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetCountLimit(1)
	e3:SetRange(LOCATION_SZONE)
	e3:SetOperation(c51670553.desop)
	c:RegisterEffect(e3)
end
function c51670553.filter(c)
	return c:GetEquipCount()~=0 and c:GetEquipGroup():IsExists(Card.IsSetCard,1,nil,0x60)
end
function c51670553.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c51670553.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c51670553.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	e:GetHandler():SetTurnCounter(0)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c51670553.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c51670553.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
	end
end
function c51670553.skipcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetFirstCardTarget()
	return tc and ep~=tp and r==REASON_BATTLE and (Duel.GetAttacker()==tc or Duel.GetAttackTarget()==tc)
end
function c51670553.skipop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_SZONE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(0,1)
	e1:SetCode(EFFECT_SKIP_DP)
	e1:SetCondition(c51670553.skipdp)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DRAW+RESET_OPPO_TURN)
	e:GetHandler():RegisterEffect(e1)
end
function c51670553.skipdp(e)
	local tc=e:GetHandler():GetFirstCardTarget()
	return tc and c51670553.filter(tc)
end
function c51670553.desop(e,tp,eg,ep,ev,re,r,rp)
	if tp~=Duel.GetTurnPlayer() then return end
	local c=e:GetHandler()
	local ct=c:GetTurnCounter()
	ct=ct+1
	c:SetTurnCounter(ct)
	if ct==2 then
		Duel.Destroy(c,REASON_RULE)
	end
end
