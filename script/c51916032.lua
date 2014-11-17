--人造人間-サイコ·ジャッカー
function c51916032.initial_effect(c)
	--code
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CHANGE_CODE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE+LOCATION_GRAVE)
	e1:SetValue(77585513)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(51916032,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOHAND+CATEGORY_SEARCH)
	e2:SetCountLimit(1,51916032)
	e2:SetCost(c51916032.cost)
	e2:SetTarget(c51916032.target)
	e2:SetOperation(c51916032.operation)
	c:RegisterEffect(e2)
end
function c51916032.filter(c)
	return c:IsSetCard(0xbc) and c:IsType(TYPE_MONSTER) and not c:IsCode(51916032) and c:IsAbleToHand()
end
function c51916032.spfilter(c,e,tp)
	return c:IsSetCard(0xbc) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c51916032.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c51916032.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c51916032.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c51916032.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c51916032.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 and Duel.SendtoHand(g,nil,REASON_EFFECT)>0 then
		Duel.ConfirmCards(1-tp,g)
		Duel.ShuffleHand(tp)
		if Duel.IsExistingMatchingCard(Card.IsFacedown,tp,0,LOCATION_SZONE,1,nil) then
			Duel.BreakEffect()
			local sg=Duel.GetMatchingGroup(Card.IsFacedown,tp,0,LOCATION_SZONE,nil)
			Duel.ConfirmCards(tp,sg)
			local trapct=sg:FilterCount(Card.IsType,nil,TYPE_TRAP)
			if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
			local spg=Duel.GetMatchingGroup(c51916032.spfilter,tp,LOCATION_HAND,0,nil,e,tp)
			if trapct>0 and spg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(51916032,1)) then
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
				local spc=Duel.SelectMatchingCard(tp,c51916032.spfilter,tp,LOCATION_HAND,0,1,trapct,nil,e,tp)
				if spc:GetCount()>0 then
					Duel.SpecialSummon(spc,0,tp,tp,false,false,POS_FACEUP)
				end
			end   
		end
	end
end
