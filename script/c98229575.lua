--U.A. Playmaker
function c98229575.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,98229575)
	e1:SetCondition(c98229575.spcon)
	e1:SetOperation(c98229575.spop)
	c:RegisterEffect(e1)
	--atk
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_ATKCHANGE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c98229575.atkcon)
	e2:SetOperation(c98229575.atkop)
	c:RegisterEffect(e2)
end
function c98229575.spfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xb2) and not c:IsCode(98229575) and c:IsAbleToHandAsCost()
end
function c98229575.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c98229575.spfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end
function c98229575.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectMatchingCard(tp,c98229575.spfilter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SendtoHand(g,nil,REASON_COST)
end
function c98229575.atkcon(e,tp,eg,ep,ev,re,r,rp)
	local at=Duel.GetAttacker()
	return at:IsSetCard(0xb2) and at:IsControler(tp) and at~=e:GetHandler()
end
function c98229575.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local at=Duel.GetAttacker()
	if c:IsFaceup() and c:IsRelateToEffect(e) and at:IsFaceup() and at:IsRelateToBattle() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(-800)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetValue(800)
		at:RegisterEffect(e2)
	end
end
