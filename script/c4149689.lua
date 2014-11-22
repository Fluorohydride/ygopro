--天狗のうちわ
function c4149689.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCode(EFFECT_CANNOT_ACTIVATE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(1,1)
	e2:SetValue(c4149689.aclimit)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(4149689,0))
	e3:SetCategory(CATEGORY_DESTROY)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTarget(c4149689.destg)
	e3:SetOperation(c4149689.desop)
	c:RegisterEffect(e3)
end
function c4149689.aclimit(e,re,tp)
	local rc=re:GetHandler()
	return re:IsActiveType(TYPE_MONSTER) and bit.band(rc:GetSummonType(),SUMMON_TYPE_FLIP)==SUMMON_TYPE_FLIP and rc:IsLevelBelow(2)
end
function c4149689.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:GetFirst():IsLevelBelow(2) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
end
function c4149689.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsLevelBelow(2) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
