--キング・オブ・ビースト
function c67757079.initial_effect(c)
	c:SetUniqueOnField(1,1,67757079)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(67757079,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND+LOCATION_GRAVE)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetCost(c67757079.spcost)
	e1:SetTarget(c67757079.sptg)
	e1:SetOperation(c67757079.spop)
	c:RegisterEffect(e1)
end
function c67757079.cfilter(c)
	return c:IsFaceup() and c:GetCode()==94878265
end
function c67757079.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c67757079.cfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c67757079.cfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c67757079.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c67757079.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
