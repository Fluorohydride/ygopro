--ラッコアラ
function c71759912.initial_effect(c)
	--atk up
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(71759912,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c71759912.condition)
	e1:SetTarget(c71759912.target)
	e1:SetOperation(c71759912.operation)
	c:RegisterEffect(e1)
end
function c71759912.cfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_BEAST)
end
function c71759912.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c71759912.cfilter,tp,LOCATION_MZONE,0,1,e:GetHandler())
end
function c71759912.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsFaceup() and chkc:IsControler(tp) and chkc:GetLocation()==LOCATION_MZONE end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFaceup,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,LOCATION_MZONE,0,1,1,nil)
end
function c71759912.operation(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsExistingMatchingCard(c71759912.cfilter,tp,LOCATION_MZONE,0,1,e:GetHandler()) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(1000)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
	end
end
