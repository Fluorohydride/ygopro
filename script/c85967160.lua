--アロマージ－ベルガモット
function c85967160.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCondition(c85967160.pccon)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsRace,RACE_PLANT))
	c:RegisterEffect(e1)
	--atk & def
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DEFCHANGE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_RECOVER)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c85967160.adcon)
	e2:SetTarget(c85967160.adtg)
	e2:SetOperation(c85967160.adop)
	c:RegisterEffect(e2)
end
function c85967160.pccon(e)
	local tp=e:GetHandlerPlayer()
	return Duel.GetLP(tp)>Duel.GetLP(1-tp)
end
function c85967160.adcon(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp
end
function c85967160.adtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return ep==tp and e:GetHandler():IsRelateToEffect(e) end
end
function c85967160.adop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(1000)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+RESET_END+RESET_OPPO_TURN)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_DEFENCE)
		c:RegisterEffect(e2)
	end
end
