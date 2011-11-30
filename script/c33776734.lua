--ハネクリボー LV9
function c33776734.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(33776734,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c33776734.spcon)
	e1:SetTarget(c33776734.sptg)
	e1:SetOperation(c33776734.spop)
	c:RegisterEffect(e1)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_TO_GRAVE_REDIRECT)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_SZONE,LOCATION_SZONE)
	e2:SetTarget(c33776734.rmtarget)
	e2:SetValue(LOCATION_REMOVED)
	c:RegisterEffect(e2)
	--atk,def
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetCode(EFFECT_SET_ATTACK)
	e3:SetRange(LOCATION_MZONE)
	e3:SetValue(c33776734.val)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_SET_DEFENCE)
	c:RegisterEffect(e4)
	--
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_CANNOT_SUMMON)
	e5:SetCondition(c33776734.excon)
	c:RegisterEffect(e5)
	local e6=e5:Clone()
	e6:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e6)
	local e7=Effect.CreateEffect(c)
	e7:SetType(EFFECT_TYPE_SINGLE)
	e7:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e7:SetCode(EFFECT_SPSUMMON_CONDITION)
	e7:SetValue(c33776734.splimit)
	c:RegisterEffect(e7)
	local e8=Effect.CreateEffect(c)
	e8:SetType(EFFECT_TYPE_SINGLE)
	e8:SetCode(EFFECT_SELF_DESTROY)
	e8:SetCondition(c33776734.descon)
	c:RegisterEffect(e8)
end
c33776734.lvupcount=1
c33776734.lvup={33776734}
function c33776734.spcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentChain()>=2
end
function c33776734.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,33776734)==0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.ConfirmCards(1-tp,e:GetHandler())
	Duel.ShuffleHand(tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
	Duel.RegisterFlagEffect(tp,33776734,RESET_CHAIN,0,0)
end
function c33776734.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c33776734.rmtarget(e,c)
	local ty=c:GetOriginalType()
	return bit.band(ty,TYPE_SPELL)~=0 and c:IsFaceup() and not c:IsStatus(STATUS_ACTIVATE_DISABLED)
end
function c33776734.val(e,c)
	return Duel.GetMatchingGroupCount(Card.IsType,c:GetControler(),0,LOCATION_GRAVE,nil,TYPE_SPELL)*500
end
function c33776734.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==33776734 and (fid==nil or c:GetFieldID()<fid)
end
function c33776734.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c33776734.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c33776734.splimit(e,se,sp,st,spos,tgp)
	return not Duel.IsExistingMatchingCard(c33776734.exfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c33776734.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c33776734.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil,c:GetFieldID())
end
