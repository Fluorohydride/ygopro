--テュアラティン
function c27769400.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(27769400,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(27769400)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c27769400.spcon)
	e1:SetTarget(c27769400.sptg)
	e1:SetOperation(c27769400.spop)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(27769400,1))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c27769400.descon)
	e2:SetTarget(c27769400.destg)
	e2:SetOperation(c27769400.desop)
	c:RegisterEffect(e2)
	if not c27769400.global_check then
		c27769400.global_check=true
		c27769400[0]=Group.CreateGroup()
		c27769400[0]:KeepAlive()
		c27769400[1]=0
		local ge1=Effect.GlobalEffect()
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_PHASE_START+PHASE_BATTLE)
		ge1:SetOperation(c27769400.checkop1)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.GlobalEffect()
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_BATTLE_DESTROYED)
		ge2:SetOperation(c27769400.checkop2)
		Duel.RegisterEffect(ge2,0)
	end
end
function c27769400.checkop1(e,tp,eg,ep,ev,re,r,rp)
	c27769400[0]:Clear()
	c27769400[0]:Merge(Duel.GetFieldGroup(Duel.GetTurnPlayer(),0,LOCATION_MZONE))
	c27769400[1]=c27769400[0]:GetCount()
end
function c27769400.checkop2(e,tp,eg,ep,ev,re,r,rp)
	if c27769400[1]<2 or c27769400[0]:GetCount()==0 then return end
	local g=eg:Filter(Card.IsLocation,nil,LOCATION_GRAVE)
	c27769400[0]:Sub(g)
	if c27769400[0]:GetCount()==0 then
		Duel.RaiseEvent(e:GetHandler(),27769400,e,0,0,0,0)
	end
end
function c27769400.spcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c27769400.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c27769400.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,1,tp,tp,false,false,POS_FACEUP)
	end
end
function c27769400.descon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SPECIAL+1
end
function c27769400.desfilter(c,att)
	return c:IsFaceup() and c:IsAttribute(att)
end
function c27769400.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,563)
	local rc=Duel.AnnounceAttribute(tp,1,0xff)
	Duel.SetTargetParam(rc)
	e:GetHandler():SetHint(CHINT_ATTRIBUTE,rc)
	local g=Duel.GetMatchingGroup(c27769400.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,rc)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c27769400.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local rc=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
	local g=Duel.GetMatchingGroup(c27769400.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,rc)
	Duel.Destroy(g,REASON_EFFECT)
	if c:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_CANNOT_SUMMON)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetTargetRange(0,1)
		e1:SetTarget(c27769400.sumlimit)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetLabel(rc)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
		c:RegisterEffect(e2)
	end
end
function c27769400.sumlimit(e,c)
	return c:IsAttribute(e:GetLabel())
end
