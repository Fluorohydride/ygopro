--サイボーグドクター
function c51020079.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(51020079,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c51020079.sptg)
	e1:SetOperation(c51020079.spop)
	c:RegisterEffect(e1)
end
function c51020079.rfilter(c,e,tp)
	return c:IsType(TYPE_TUNER) and Duel.IsExistingTarget(c51020079.spfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp,c:GetLevel(),c:GetAttribute())
end
function c51020079.spfilter(c,e,tp,lv,att)
	return c:GetLevel()==lv and c:IsAttribute(att) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c51020079.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c51020079.spfilter(chkc,e,tp) end
	if chk==0 then return Duel.CheckReleaseGroup(tp,c51020079.rfilter,1,nil,e,tp) end
	local rg=Duel.SelectReleaseGroup(tp,c51020079.rfilter,1,1,nil,e,tp)
	local r=rg:GetFirst()
	local lv=r:GetLevel()
	local att=r:GetAttribute()
	Duel.Release(rg,REASON_COST)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c51020079.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp,lv,att)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c51020079.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
