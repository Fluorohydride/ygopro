--武器庫荒らし
function c55348096.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c55348096.target)
	e1:SetOperation(c55348096.activate)
	c:RegisterEffect(e1)
end
function c55348096.tgfilter(c)
	return c:IsType(TYPE_EQUIP) and c:IsAbleToGrave()
end
function c55348096.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)>0 end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,1-tp,LOCATION_DECK)
end
function c55348096.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(1-tp,c55348096.tgfilter,1-tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	else
		local cg=Duel.GetFieldGroup(1-tp,LOCATION_DECK,0)
		Duel.ConfirmCards(tp,cg)
		Duel.ShuffleDeck(1-tp)
	end
end
