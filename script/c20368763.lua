--幻獣機ハリアード
function c20368763.initial_effect(c)
	--level
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_LEVEL)
	e1:SetValue(c20368763.lvval)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetCondition(c20368763.indcon)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	c:RegisterEffect(e3)
	--token
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(20368763,0))
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e4:SetCode(EVENT_RELEASE)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCountLimit(1)
	e4:SetCondition(c20368763.spcon)
	e4:SetTarget(c20368763.sptg)
	e4:SetOperation(c20368763.spop)
	c:RegisterEffect(e4)
	--destroy
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(20368763,1))
	e5:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e5:SetType(EFFECT_TYPE_IGNITION)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCountLimit(1)
	e5:SetCost(c20368763.spcost2)
	e5:SetTarget(c20368763.sptg2)
	e5:SetOperation(c20368763.spop2)
	c:RegisterEffect(e5)
end
function c20368763.lvval(e,c)
	local tp=c:GetControler()
	local lv=0
	for i=0,4 do
		local tc=Duel.GetFieldCard(tp,LOCATION_MZONE,i)
		if tc and tc:IsCode(31533705) then lv=lv+tc:GetLevel() end
	end
	return lv
end
function c20368763.indcon(e)
	return Duel.IsExistingMatchingCard(Card.IsType,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil,TYPE_TOKEN)
end
function c20368763.cfilter(c,tp)
	return c:IsPreviousLocation(LOCATION_MZONE) and c:GetPreviousControler()==tp and c:IsReason(REASON_COST)
end
function c20368763.spcon(e,tp,eg,ep,ev,re,r,rp)
	return re and re:GetHandler()~=e:GetHandler() and re:IsHasType(0x7f0) and eg:IsExists(c20368763.cfilter,1,nil,tp)
end
function c20368763.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c20368763.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	if Duel.IsPlayerCanSpecialSummonMonster(tp,31533705,0x101b,0x4011,0,0,3,RACE_MACHINE,ATTRIBUTE_WIND) then
		local token=Duel.CreateToken(tp,20368764)
		Duel.SpecialSummon(token,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c20368763.spcost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsType,1,nil,TYPE_TOKEN) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsType,1,1,nil,TYPE_TOKEN)
	Duel.Release(g,REASON_COST)
end
function c20368763.spfilter(c,e,tp)
	return c:IsSetCard(0x101b) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c20368763.sptg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c20368763.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c20368763.spop2(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c20368763.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
