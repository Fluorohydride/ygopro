--エーリアン・ウォリアー
function c98719226.initial_effect(c)
	--counter
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(98719226,0))
	e1:SetCategory(CATEGORY_COUNTER)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c98719226.ctcon)
	e1:SetOperation(c98719226.ctop)
	c:RegisterEffect(e1)
	--atk def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_DAMAGE_CALCULATING)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c98719226.adval)
	c:RegisterEffect(e2)
end
function c98719226.ctcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():IsReason(REASON_BATTLE)
end
function c98719226.ctop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetReasonCard()
	if tc:IsFaceup() and tc:IsRelateToBattle() then
		tc:AddCounter(0xe,2)
	end
end
function c98719226.addown(c,e)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetReset(RESET_PHASE+RESET_DAMAGE_CAL)
	e1:SetValue(c:GetCounter(0xe)*-300)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e2)
end
function c98719226.adval(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if not d then return end
	if a:GetCounter(0xe)>0 and d:IsSetCard(0xc) then c98719226.addown(a,e) end
	if d:GetCounter(0xe)>0 and a:IsSetCard(0xc) then c98719226.addown(d,e) end
end
