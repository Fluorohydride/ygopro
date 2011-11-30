--D－HERO ディアボリックガイ
function c9411399.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(9411399,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCost(c9411399.cost)
	e1:SetTarget(c9411399.target)
	e1:SetOperation(c9411399.operation)
	c:RegisterEffect(e1)
end
function c9411399.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c9411399.filter(c,e,sp)
	return c:GetCode()==9411399 and c:IsCanBeSpecialSummoned(e,0,sp,false,false)
end
function c9411399.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c9411399.filter,tp,LOCATION_DECK,0,1,nil,e,tp)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c9411399.operation(e,tp,eg,ep,ev,re,r,rp)
	local sc=Duel.GetFirstMatchingCard(c9411399.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if sc~=nil then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
			Duel.SpecialSummon(sc,0,tp,tp,false,false,POS_FACEUP)
		else
			Duel.Destroy(sc,REASON_EFFECT)
		end
		Duel.ShuffleDeck(tp)
	end
end
