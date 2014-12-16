--キューキューロイド
function c36378213.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(36378213,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c36378213.target)
	e1:SetOperation(c36378213.activate)
	c:RegisterEffect(e1)
end
function c36378213.filter(c,e,tp)
	return c:IsPreviousLocation(LOCATION_GRAVE) and c:GetPreviousControler()==tp and c:IsSetCard(0x16)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c36378213.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local g=eg:Filter(c36378213.filter,nil,e,tp)
	if chk==0 then return g:GetCount()~=0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c36378213.spfilter(c,e,tp)
	return c:IsPreviousLocation(LOCATION_GRAVE) and c:GetPreviousControler()==tp and c:IsSetCard(0x16)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and c:IsRelateToEffect(e)
end
function c36378213.activate(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	local g=eg:Filter(c36378213.spfilter,nil,e,tp)
	if g:GetCount()==0 then return end
	if g:GetCount()>1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		g=g:Select(tp,1,ft,nil)
	end
	Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
end
