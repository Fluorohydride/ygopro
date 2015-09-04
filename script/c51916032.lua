--人造人間－サイコ・ジャッカー
function c51916032.initial_effect(c)
	--change
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_CHANGE_CODE)
	e1:SetRange(LOCATION_MZONE+LOCATION_GRAVE)
	e1:SetValue(77585513)
	c:RegisterEffect(e1)
	--tohand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(51916032,0))
	e2:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH+CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1,51916032)
	e2:SetCost(c51916032.cost)
	e2:SetTarget(c51916032.target)
	e2:SetOperation(c51916032.operation)
	c:RegisterEffect(e2)
end
function c51916032.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c51916032.filter(c)
	return c:IsSetCard(0xbc) and not c:IsCode(51916032) and c:IsAbleToHand()
end
function c51916032.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c51916032.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c51916032.cffilter(c)
	return c:IsFacedown() and c:GetSequence()<5
end
function c51916032.spfilter(c,e,tp)
	return c:IsSetCard(0xbc) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c51916032.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c51916032.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()==0 or Duel.SendtoHand(g,nil,REASON_EFFECT)==0 then return end
	Duel.ConfirmCards(1-tp,g)
	Duel.ShuffleHand(tp)
	local cg=Duel.GetMatchingGroup(c51916032.cffilter,tp,0,LOCATION_SZONE,nil)
	if cg:GetCount()>0 then
		Duel.BreakEffect()
		Duel.ConfirmCards(tp,cg)
		local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
		if ft<=0 then return end
		local ct=cg:FilterCount(Card.IsType,nil,TYPE_TRAP)
		if ct>ft then ct=ft end
		local sg=Duel.GetMatchingGroup(c51916032.spfilter,tp,LOCATION_HAND,0,nil,e,tp)
		if ct>0 and sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(51916032,1)) then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local tg=sg:Select(tp,1,ct,nil)
			Duel.SpecialSummon(tg,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
