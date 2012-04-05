--D－HERO ダイハードガイ
function c77608643.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetOperation(c77608643.operation)
	c:RegisterEffect(e1)
	local g=Group.CreateGroup()
	g:KeepAlive()
	e1:SetLabelObject(g)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(77608643,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCondition(c77608643.spcon)
	e2:SetTarget(c77608643.sptg)
	e2:SetOperation(c77608643.spop)
	e2:SetLabelObject(g)
	c:RegisterEffect(e2)
end
function c77608643.filter(c,e,tp)
	return c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_BATTLE)
		and c:GetPreviousControler()==tp and c:IsSetCard(0xc008)
end
function c77608643.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c77608643.filter,nil,e,tp)
	if g:GetCount()==0 then return end
	local sg=e:GetLabelObject()
	local c=e:GetHandler()
	if c:GetFlagEffect(77608643)==0 then
		sg:Clear()
		c:RegisterFlagEffect(77608643,RESET_EVENT+0x3fe0000+RESET_PHASE+PHASE_STANDBY+RESET_SELF_TURN,0,1)
	end
	sg:Merge(g)
end
function c77608643.spcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and e:GetHandler():GetFlagEffect(77608643)~=0
end
function c77608643.spfilter(c,e,tp)
	return c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_BATTLE) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c77608643.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetLabelObject():IsExists(c77608643.spfilter,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_GRAVE)
end
function c77608643.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=e:GetLabelObject():FilterSelect(tp,c77608643.spfilter,1,1,nil,e,tp)
	if g:GetCount()~=0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
