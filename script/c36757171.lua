--ヴェルズ・バハムート
function c36757171.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterBoolFunction(Card.IsSetCard,0xa),4,2)
	c:EnableReviveLimit()
	--attack up
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_CONTROL+CATEGORY_HANDES)
	e1:SetDescription(aux.Stringid(36757171,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c36757171.cost)
	e1:SetTarget(c36757171.target)
	e1:SetOperation(c36757171.operation)
	c:RegisterEffect(e1)
end
function c36757171.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c36757171.filter(c)
	return c:IsFaceup() and c:IsControlerCanBeChanged()
end
function c36757171.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c36757171.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c36757171.filter,tp,0,LOCATION_MZONE,1,nil)
		and Duel.IsExistingMatchingCard(c36757171.dfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c36757171.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,1)
end
function c36757171.dfilter(c)
	return c:IsSetCard(0xa) and c:IsType(TYPE_MONSTER) and c:IsDiscardable()
end
function c36757171.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
	local g=Duel.SelectMatchingCard(tp,c36757171.dfilter,tp,LOCATION_HAND,0,1,1,nil)
	if g:GetCount()==0 then return end
	Duel.SendtoGrave(g,REASON_EFFECT+REASON_DISCARD)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and not Duel.GetControl(tc,tp) then
		if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	end
end
