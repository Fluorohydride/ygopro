--輝光帝ギャラクシオン
function c40390147.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterBoolFunction(Card.IsSetCard,0x55),4,2)
	c:EnableReviveLimit()
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(40390147,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c40390147.sptg)
	e1:SetOperation(c40390147.spop)
	c:RegisterEffect(e1)
end
function c40390147.spfilter(c,e,tp)
	return c:IsCode(93717133) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c40390147.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local b1=Duel.IsExistingMatchingCard(c40390147.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) and e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST)
	local b2=Duel.IsExistingMatchingCard(c40390147.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp) and e:GetHandler():CheckRemoveOverlayCard(tp,2,REASON_COST)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and (b1 or b2) end
	if b1 and b2 then
		local opt=Duel.SelectOption(tp,aux.Stringid(40390147,1),aux.Stringid(40390147,2))
		e:SetLabel(opt)
		e:GetHandler():RemoveOverlayCard(tp,opt+1,opt+1,REASON_COST)
	elseif b1 then
		Duel.SelectOption(tp,aux.Stringid(40390147,1))
		e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
		e:SetLabel(0)
	else
		Duel.SelectOption(tp,aux.Stringid(40390147,2))
		e:GetHandler():RemoveOverlayCard(tp,2,2,REASON_COST)
		e:SetLabel(1)
	end
end
function c40390147.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=nil
	if e:GetLabel()==0 then
		tc=Duel.GetFirstMatchingCard(c40390147.spfilter,tp,LOCATION_HAND,0,nil,e,tp)
	else
		tc=Duel.GetFirstMatchingCard(c40390147.spfilter,tp,LOCATION_DECK,0,nil,e,tp)
	end
	if tc then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
