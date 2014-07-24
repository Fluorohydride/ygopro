--クリフォート・ゲノム
function c37991342.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--splimit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetTargetRange(1,0)
	e2:SetCondition(c37991342.con)
	e2:SetTarget(c37991342.splimit)
	c:RegisterEffect(e2)
	--atk
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_UPDATE_ATTACK)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(0,LOCATION_MZONE)
	e3:SetCondition(c37991342.con)
	e3:SetValue(-300)
	c:RegisterEffect(e3)	
	--summon with no tribute
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(37991342,0))
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_SUMMON_PROC)
	e4:SetCondition(c37991342.ntcon)
	c:RegisterEffect(e4)
	--act limit
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e5:SetCode(EVENT_SUMMON_SUCCESS)
	e5:SetOperation(c37991342.regop)
	c:RegisterEffect(e5)
	local e6=e5:Clone()
	e6:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e6)
	--destroy
	local e8=Effect.CreateEffect(c)
	e8:SetDescription(aux.Stringid(37991342,1))
	e8:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e8:SetCategory(CATEGORY_DESTROY)
	e8:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DELAY+EFFECT_FLAG_DAMAGE_STEP)
	e8:SetCode(EVENT_RELEASE)
	e8:SetTarget(c37991342.destg)
	e8:SetOperation(c37991342.desop)
	c:RegisterEffect(e8)
end
function c37991342.con(e,tp,eg,ep,ev,re,r,rp)
	local seq=e:GetHandler():GetSequence()
	return seq==6 or seq==7
end
function c37991342.splimit(e,c,tp,sumtp,sumpos)
	return not c:IsSetCard(0xaa)
end
function c37991342.ntcon(e,c)
	if c==nil then return true end
	return c:GetLevel()>4 and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
function c37991342.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_NORMAL)==SUMMON_TYPE_NORMAL then 
		--immune 	
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_IMMUNE_EFFECT)
		e1:SetReset(RESET_EVENT+0x5ee0000)
		e1:SetValue(c37991342.efilter)
		c:RegisterEffect(e1)
		if c:GetSummonType()==SUMMON_TYPE_ADVANCE then return end
	end
	--search
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetReset(RESET_EVENT+0x1ff0000)
	e2:SetCode(EFFECT_CHANGE_LEVEL)
	e2:SetValue(4)
	c:RegisterEffect(e2)
	--search
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetReset(RESET_EVENT+0x1ff0000)
	e3:SetCode(EFFECT_SET_BASE_ATTACK)
	e3:SetValue(1800)
	c:RegisterEffect(e3)
end
function c37991342.efilter(e,te)
	local lv=e:GetHandler():GetLevel()
	return te:IsActiveType(TYPE_MONSTER) 
		and te:IsHasType(EFFECT_TYPE_ACTIONS) 
		and (te:GetHandler():GetOriginalLevel()<lv or te:GetHandler():GetOriginalRank()<lv)
end
function c37991342.desfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c37991342.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c37991342.desfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c37991342.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c37991342.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c37991342.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
