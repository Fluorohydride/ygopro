--墓守の大神官
function c3825890.initial_effect(c)
	--summon with 1 tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(3825890,0))
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c3825890.otcon)
	e1:SetOperation(c3825890.otop)
	e1:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e1)
	--atk up
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetValue(c3825890.atkval)
	c:RegisterEffect(e2)
	--Destroy replace
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_DESTROY_REPLACE)
	e3:SetTarget(c3825890.desreptg)
	c:RegisterEffect(e3)
end
function c3825890.otfilter(c,tp)
	return c:IsSetCard(0x2e) and (c:IsControler(tp) or c:IsFaceup())
end
function c3825890.otcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local mg=Duel.GetMatchingGroup(c3825890.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	return c:GetLevel()>6 and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.GetTributeCount(c,mg)>0
end
function c3825890.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local mg=Duel.GetMatchingGroup(c3825890.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	local sg=Duel.SelectTribute(tp,c,1,1,mg)
	c:SetMaterial(sg)
	Duel.Release(sg, REASON_SUMMON+REASON_MATERIAL)
end
function c3825890.filter(c)
	return c:IsSetCard(0x2e) and c:IsType(TYPE_MONSTER)
end
function c3825890.atkval(e,c)
	return Duel.GetMatchingGroupCount(c3825890.filter,c:GetControler(),LOCATION_GRAVE,0,nil)*200
end
function c3825890.desreptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return not c:IsReason(REASON_REPLACE)
		and Duel.IsExistingMatchingCard(c3825890.filter,tp,LOCATION_HAND,0,1,nil) end
	if Duel.SelectYesNo(tp,aux.Stringid(3825890,1)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
		local g=Duel.SelectMatchingCard(tp,c3825890.filter,tp,LOCATION_HAND,0,1,1,nil)
		Duel.SendtoGrave(g,REASON_EFFECT+REASON_REPLACE)
		return true
	else return false end
end
