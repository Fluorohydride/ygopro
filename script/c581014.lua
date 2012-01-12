--ダイガスタ·エメラル
function c581014.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterEqualFunction(Card.GetLevel,4),2)
	c:EnableReviveLimit()
	--sel effect
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(581014,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c581014.target)
	e1:SetOperation(c581014.operation)
	c:RegisterEffect(e1)
end
function c581014.filter1(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToDeck()
end
function c581014.filter2(c,e,tp)
	return not c:IsType(TYPE_EFFECT) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c581014.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		if e:GetLabel()==0 then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c581014.filter1(c)
		else return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c581014.filter2(c,e,tp) end
	end
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST)
		and ((Duel.IsPlayerCanDraw(tp,1) and Duel.IsExistingTarget(c581014.filter1,tp,LOCATION_GRAVE,0,3,nil))
		or Duel.IsExistingTarget(c581014.filter2,tp,LOCATION_GRAVE,0,1,nil,e,tp)) end
	local op=0
	local b1=Duel.IsPlayerCanDraw(tp,1) and Duel.IsExistingTarget(c581014.filter1,tp,LOCATION_GRAVE,0,3,nil)
	local b2=Duel.IsExistingTarget(c581014.filter2,tp,LOCATION_GRAVE,0,1,nil,e,tp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(581014,0))
	if b1 and b2 then
		op=Duel.SelectOption(tp,aux.Stringid(581014,1),aux.Stringid(581014,2))
	elseif b1 then
		op=Duel.SelectOption(tp,aux.Stringid(581014,1))
	else op=Duel.SelectOption(tp,aux.Stringid(581014,2))+1 end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
	if op==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local g=Duel.SelectTarget(tp,c581014.filter1,tp,LOCATION_GRAVE,0,3,3,nil)
		e:SetCategory(CATEGORY_TODECK+CATEGORY_DRAW)
		Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectTarget(tp,c581014.filter2,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
	end
	e:SetLabel(op)
end
function c581014.tgfilter(c,e)
	return not c:IsRelateToEffect(e)
end
function c581014.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
		if tg:IsExists(c581014.tgfilter,1,nil,e) then return end
		Duel.SendtoDeck(tg,nil,0,REASON_EFFECT)
		Duel.ShuffleDeck(tp)
		Duel.BreakEffect()
		Duel.Draw(tp,1,REASON_EFFECT)
	else
		local tc=Duel.GetFirstTarget()
		if tc:IsRelateToEffect(e) then
			Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
