--ナチュルの神星樹
function c3734202.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c3734202.target)
	e1:SetOperation(c3734202.activate)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(3734202,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetCountLimit(1,3734202)
	e2:SetCost(c3734202.spcost1)
	e2:SetTarget(c3734202.sptg1)
	e2:SetOperation(c3734202.spop1)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(3734202,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetCountLimit(1,3734202)
	e3:SetCost(c3734202.spcost2)
	e3:SetTarget(c3734202.sptg2)
	e3:SetOperation(c3734202.spop2)
	c:RegisterEffect(e3)
	--search
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(3734202,2))
	e4:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_TO_GRAVE)
	e4:SetCondition(c3734202.thcon)
	e4:SetTarget(c3734202.thtg)
	e4:SetOperation(c3734202.thop)
	c:RegisterEffect(e4)
end
function c3734202.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local b1=Duel.CheckReleaseGroup(tp,c3734202.cfilter1,1,nil)
		and Duel.IsExistingMatchingCard(c3734202.spfilter1,tp,LOCATION_DECK,0,1,nil,e,tp)
	local b2=Duel.CheckReleaseGroup(tp,c3734202.cfilter2,1,nil)
		and Duel.IsExistingMatchingCard(c3734202.spfilter2,tp,LOCATION_DECK,0,1,nil,e,tp)
	if Duel.GetFlagEffect(tp,3734202)==0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and (b1 or b2) and Duel.SelectYesNo(tp,aux.Stringid(3734202,3)) then
		local opt=0
		if b1 and b2 then
			opt=Duel.SelectOption(tp,aux.Stringid(3734202,0),aux.Stringid(3734202,1))
		elseif b1 then
			opt=Duel.SelectOption(tp,aux.Stringid(3734202,0))
		else
			opt=Duel.SelectOption(tp,aux.Stringid(3734202,1))+1
		end
		if opt==0 then
			local g=Duel.SelectReleaseGroup(tp,c3734202.cfilter1,1,1,nil)
			Duel.Release(g,REASON_COST)
		else
			local g=Duel.SelectReleaseGroup(tp,c3734202.cfilter2,1,1,nil)
			Duel.Release(g,REASON_COST)
		end
		Duel.RegisterFlagEffect(tp,3734202,RESET_PHASE+PHASE_END,0,1)
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
		e:SetLabel(opt+1)
	else
		e:SetCategory(0)
		e:SetLabel(0)
	end
end
function c3734202.activate(e,tp,eg,ep,ev,re,r,rp)
	local opt=e:GetLabel()
	if opt==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	if opt==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c3734202.spfilter1,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		if g:GetCount()>0 then
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		end
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c3734202.spfilter2,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		if g:GetCount()>0 then
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
function c3734202.cfilter1(c)
	return c:IsRace(RACE_INSECT) and c:IsAttribute(ATTRIBUTE_EARTH)
end
function c3734202.spcost1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,3734202)==0
		and Duel.CheckReleaseGroup(tp,c3734202.cfilter1,1,nil) end
	Duel.RegisterFlagEffect(tp,3734202,RESET_PHASE+PHASE_END,0,1)
	local g=Duel.SelectReleaseGroup(tp,c3734202.cfilter1,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c3734202.spfilter1(c,e,tp)
	return c:IsLevelBelow(4) and c:IsRace(RACE_PLANT) and c:IsAttribute(ATTRIBUTE_EARTH)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c3734202.sptg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c3734202.spfilter1,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c3734202.spop1(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c3734202.spfilter1,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c3734202.cfilter2(c)
	return c:IsRace(RACE_PLANT) and c:IsAttribute(ATTRIBUTE_EARTH)
end
function c3734202.spcost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,3734202)==0
		and Duel.CheckReleaseGroup(tp,c3734202.cfilter2,1,nil) end
	Duel.RegisterFlagEffect(tp,3734202,RESET_PHASE+PHASE_END,0,1)
	local g=Duel.SelectReleaseGroup(tp,c3734202.cfilter2,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c3734202.spfilter2(c,e,tp)
	return c:IsLevelBelow(4) and c:IsRace(RACE_INSECT) and c:IsAttribute(ATTRIBUTE_EARTH)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c3734202.sptg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c3734202.spfilter2,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c3734202.spop2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c3734202.spfilter2,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c3734202.thcon(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsReason(REASON_RETURN)
end
function c3734202.thfilter(c)
	return c:IsSetCard(0x2a) and not c:IsCode(3734202) and c:IsAbleToHand()
end
function c3734202.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c3734202.thop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c3734202.thfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
