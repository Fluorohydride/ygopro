--神竜アクアバザル
function c67964209.initial_effect(c)
	--todeck
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(67964209,0))
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c67964209.tdcost)
	e1:SetTarget(c67964209.tdtg)
	e1:SetOperation(c67964209.tdop)
	c:RegisterEffect(e1)
end
function c67964209.cfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WATER)
end
function c67964209.tdcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c67964209.cfilter,1,e:GetHandler()) end
	local g=Duel.SelectReleaseGroup(tp,c67964209.cfilter,1,1,e:GetHandler())
	Duel.Release(g,REASON_COST)
end
function c67964209.filter(c)
	local tpe=c:GetType()
	return bit.band(tpe,TYPE_SPELL)~=0 and bit.band(tpe,TYPE_CONTINUOUS+TYPE_FIELD)~=0 and c:IsAbleToDeck()
end
function c67964209.tdtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c67964209.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c67964209.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c67964209.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
end
function c67964209.tdop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoDeck(tc,nil,0,REASON_EFFECT)
	end
end
