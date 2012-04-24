--破壊神の系譜
function c29307554.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c29307554.condition)
	e1:SetTarget(c29307554.target)
	e1:SetOperation(c29307554.activate)
	c:RegisterEffect(e1)
	if not c29307554.global_check then
		c29307554.global_check=true
		c29307554[0]=true
		c29307554[1]=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_DESTROY)
		ge1:SetOperation(c29307554.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c29307554.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c29307554.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if tc:IsDefencePos() then
			c29307554[1-tc:GetControler()]=true
		end
		tc=eg:GetNext()
	end
end
function c29307554.clear(e,tp,eg,ep,ev,re,r,rp)
	c29307554[0]=false
	c29307554[1]=false
end
function c29307554.condition(e,tp,eg,ep,ev,re,r,rp)
	return c29307554[tp] and Duel.GetTurnPlayer()==tp and Duel.GetCurrentPhase()<=PHASE_BATTLE
end
function c29307554.filter(c)
	return c:IsFaceup() and c:IsLevelAbove(8) and c:GetEffectCount(EFFECT_EXTRA_ATTACK)==0
end
function c29307554.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c29307554.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c29307554.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c29307554.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c29307554.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and c29307554.filter(tc) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EXTRA_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetValue(1)
		tc:RegisterEffect(e1)
	end
end
