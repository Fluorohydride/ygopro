--ラヴァルバル·チェイン
function c34086406.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterEqualFunction(Card.GetLevel,4),2)
	c:EnableReviveLimit()
	--sel effect
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(34086406,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c34086406.cost)
	e1:SetTarget(c34086406.target)
	e1:SetOperation(c34086406.operation)
	c:RegisterEffect(e1)
end
function c34086406.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c34086406.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToGrave,tp,LOCATION_DECK,0,1,nil)
		or Duel.IsExistingMatchingCard(Card.IsType,tp,LOCATION_DECK,0,1,nil,TYPE_MONSTER) end
	local op=0
	local b1=Duel.IsExistingMatchingCard(Card.IsAbleToGrave,tp,LOCATION_DECK,0,1,nil)
	local b2=Duel.IsExistingMatchingCard(Card.IsType,tp,LOCATION_DECK,0,1,nil,TYPE_MONSTER)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(34086406,0))
	if b1 and b2 then
		op=Duel.SelectOption(tp,aux.Stringid(34086406,1),aux.Stringid(34086406,2))
	elseif b1 then
		op=Duel.SelectOption(tp,aux.Stringid(34086406,1))
	else op=Duel.SelectOption(tp,aux.Stringid(34086406,2))+1 end
	if op==0 then
		e:SetCategory(CATEGORY_TOGRAVE)
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,0,1,tp,LOCATION_DECK)
	else
		e:SetCategory(0)
	end
	e:SetLabel(op)
end
function c34086406.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=Duel.SelectMatchingCard(tp,Card.IsAbleToGrave,tp,LOCATION_DECK,0,1,1,nil)
		Duel.SendtoGrave(g,REASON_EFFECT)
	else
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(34086406,2))
		local g=Duel.SelectMatchingCard(tp,Card.IsType,tp,LOCATION_DECK,0,1,1,nil,TYPE_MONSTER)
		local tc=g:GetFirst()
		Duel.ShuffleDeck(tp)
		Duel.MoveSequence(tc,0)
		Duel.ConfirmDecktop(tp,1)
	end
end
