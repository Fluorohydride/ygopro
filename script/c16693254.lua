--リチュア・アバンス
function c16693254.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(16693254,0))
	e1:SetCategory(CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c16693254.target)
	e1:SetOperation(c16693254.operation)
	c:RegisterEffect(e1)
end
function c16693254.filter(c)
	return c:IsSetCard(0x3a) and c:IsType(TYPE_MONSTER)
end
function c16693254.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c16693254.filter,tp,LOCATION_DECK,0,1,nil) end
end
function c16693254.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(16693254,1))
	local g=Duel.SelectMatchingCard(tp,c16693254.filter,tp,LOCATION_DECK,0,1,1,nil)
	local tc=g:GetFirst()
	if tc then
		Duel.ShuffleDeck(tp)
		Duel.MoveSequence(tc,0)
		Duel.ConfirmDecktop(tp,1)
	end
end
