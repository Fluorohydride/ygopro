--ヴァイロン・マター
function c41858121.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_DRAW+CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c41858121.target)
	e1:SetOperation(c41858121.activate)
	c:RegisterEffect(e1)
end
function c41858121.filter(c)
	return c:IsType(TYPE_EQUIP) and c:IsAbleToDeck()
end
function c41858121.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c41858121.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c41858121.filter,tp,LOCATION_GRAVE,0,3,nil)	
		and ((Duel.IsPlayerCanDraw(tp) and Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>0)
		or Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil))
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c41858121.filter,tp,LOCATION_GRAVE,0,3,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,3,0,0)
end
function c41858121.tgfilter(c,e)
	return not c:IsRelateToEffect(e)
end
function c41858121.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if g:IsExists(c41858121.tgfilter,1,nil,e) then return end
	Duel.SendtoDeck(g,nil,0,REASON_EFFECT)
	Duel.ShuffleDeck(tp)
	Duel.BreakEffect()
	local op=0
	local b1=Duel.IsPlayerCanDraw(tp,1)
	local b2=Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,0)
	if b1 and b2 then op=Duel.SelectOption(tp,aux.Stringid(41858121,0),aux.Stringid(41858121,1))
	elseif b1 then op=Duel.SelectOption(tp,aux.Stringid(41858121,0))
	elseif b2 then Duel.SelectOption(tp,aux.Stringid(41858121,1)) op=1
	else return end
	if op==0 then
		Duel.Draw(tp,1,REASON_EFFECT)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local dg=Duel.SelectMatchingCard(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,1,nil)
		Duel.Destroy(dg,REASON_EFFECT)
	end
end
