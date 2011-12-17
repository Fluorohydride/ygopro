Auxiliary={}
aux=Auxiliary

function Auxiliary.Stringid(code,id)
	return code*16+id
end
function Auxiliary.Next(g)
	local first=true
	return	function()
				if first then first=false return g:GetFirst()
				else return g:GetNext() end
			end
end
function Auxiliary.NULL()
end
function Auxiliary.TRUE()
	return true
end
function Auxiliary.FALSE()
	return false
end
function Auxiliary.AND(f1,f2)
	return 	function(a,b,c)
				return f1(a,b,c) and f2(a,b,c)
			end
end
function Auxiliary.OR(f1,f2)
	return 	function(a,b,c)
				return f1(a,b,c) or f2(a,b,c)
			end
end
function Auxiliary.NOT(f)
	return 	function(a,b,c)
				return not f(a,b,c)
			end
end
function Auxiliary.IsDualState(effect)
	local c=effect:GetHandler()
	return not c:IsDisabled() and c:IsDualState()
end
function Auxiliary.IsNotDualState(effect)
	local c=effect:GetHandle()
	return c:IsDisabled() or not c:IsDualState()
end
function Auxiliary.DualNormalCondition(effect)
	local c=effect:GetHandler()
	return c:IsFaceup() and not c:IsDualState()
end
function Auxiliary.EnableDualAttribute(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DUAL_SUMMONABLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CHANGE_TYPE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE+LOCATION_GRAVE)
	e2:SetCondition(aux.DualNormalCondition)
	e2:SetValue(TYPE_NORMAL+TYPE_DUAL+TYPE_MONSTER)
	c:RegisterEffect(e2)
end
function Auxiliary.TargetEqualFunction(f,value,a,b,c)
	return 	function(effect,target)
				return f(target,a,b,c)==value 
			end
end
function Auxiliary.TargetBoolFunction(f,a,b,c)
	return 	function(effect,target)
				return f(target,a,b,c)
			end
end
function Auxiliary.FilterEqualFunction(f,value,a,b,c)
	return 	function(target)
				return f(target,a,b,c)==value 
			end
end
function Auxiliary.FilterBoolFunction(f,a,b,c)
	return 	function(target)
				return f(target,a,b,c)
			end
end
function Auxiliary.NonTuner(f,a,b,c)
	return	function(target)
				return target:IsNotTuner() and (not f or f(target,a,b,c))
			end
end
function Auxiliary.AddSynchroProcedure(c,f1,f2,ct)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(Auxiliary.SynCondition(f1,f2,ct))
	e1:SetOperation(Auxiliary.SynOperation(f1,f2,ct))
	e1:SetValue(SUMMON_TYPE_SYNCHRO)
	c:RegisterEffect(e1)
end
function Auxiliary.SynCondition(f1,f2,ct)
	return	function(e,c,tuner)
				if c==nil then return true end
				if tuner then return Duel.CheckTunerMaterial(c,tuner,f1,f2,ct) end
				return Duel.CheckSynchroMaterial(c,f1,f2,ct)
			end
end
function Auxiliary.SynOperation(f1,f2,ct)
	return	function(e,tp,eg,ep,ev,re,r,rp,c,tuner)
				local g=nil
				if tuner then
					g=Duel.SelectTunerMaterial(c:GetControler(),c,tuner,f1,f2,ct)
				else
					g=Duel.SelectSynchroMaterial(c:GetControler(),c,f1,f2,ct)
				end
				c:SetMaterial(g)
				Duel.SendtoGrave(g,REASON_MATERIAL+REASON_SYNCHRO)
			end
end
function Auxiliary.AddSynchroProcedure2(c,f1,f2)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(Auxiliary.SynCondition2(f1,f2))
	e1:SetOperation(Auxiliary.SynOperation2(f1,f2))
	e1:SetValue(SUMMON_TYPE_SYNCHRO)
	c:RegisterEffect(e1)
end
function Auxiliary.SynFilter1(c,syncard,lv,f1,f2)
	local clv=c:GetLevel()
	if clv==0 or clv>=lv then return false end
	return c:IsType(TYPE_TUNER) and c:IsCanBeSynchroMaterial(syncard) and f1(c)
		and Duel.IsExistingMatchingCard(Auxiliary.SynFilter2,syncard:GetControler(),LOCATION_MZONE,LOCATION_MZONE,1,c,syncard,lv-clv,f2)
end
function Auxiliary.SynFilter2(c,syncard,lv,f2)
	return c:IsCanBeSynchroMaterial(syncard) and c:GetLevel()==lv and f2(c)
end
function Auxiliary.SynCondition2(f1,f2)
	return	function(e,c,tuner)
				if c==nil then return true end
				local lv=c:GetLevel()
				if tuner then return Auxiliary.SynFilter1(tuner,c,lv,f1,f2) end
				return Duel.IsExistingMatchingCard(Auxiliary.SynFilter1,c:GetControler(),LOCATION_MZONE,LOCATION_MZONE,1,nil,c,lv,f1,f2)
			end
end
function Auxiliary.SynOperation2(f1,f2)
	return	function(e,tp,eg,ep,ev,re,r,rp,c,tuner)
				local g=nil
				local lv=c:GetLevel()
				if tuner then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
					g=Duel.SelectMatchingCard(tp,Auxiliary.SynFilter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,tuner,c,lv-tuner:GetLevel(),f2)
				else
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
					g=Duel.SelectMatchingCard(tp,Auxiliary.SynFilter1,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,c,lv,f1,f2)
					local sc=g:GetFirst()
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
					local g2=Duel.SelectMatchingCard(tp,Auxiliary.SynFilter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,sc,c,lv-sc:GetLevel(),f2)
					g:AddCard(g2:GetFirst())
				end
				c:SetMaterial(g)
				Duel.SendtoGrave(g,REASON_MATERIAL+REASON_SYNCHRO)
			end
end
function Auxiliary.AddXyzProcedure(c,f,ct)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(Auxiliary.ExCondition(f,ct))
	e1:SetOperation(Auxiliary.ExOperation(f,ct))
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
end
function Auxiliary.ExCondition(f,ct)
	return	function(e,c)
				if c==nil then return true end
				local g=Duel.GetXyzMaterial(c)
				return g:IsExists(f,ct,nil)
			end
end
function Auxiliary.ExOperation(f,ct)
	return	function(e,tp,eg,ep,ev,re,r,rp,c)
				local g=Duel.GetXyzMaterial(c)
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
				local mg=g:FilterSelect(tp,f,ct,ct,nil)
				Duel.Overlay(c,mg)
				c:SetMaterial(mg)
			end
end
function Auxiliary.FConditionCheckF(c,chkf)
	return c:IsOnField() and c:IsControler(chkf)
end
function Auxiliary.AddFusionProcCode2(c,code1,code2,sub,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCode2(code1,code2,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCode2(code1,code2,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilter21(c,code1,code2)
	local code=c:GetCode()
	return code==code1 or code==code2
end
function Auxiliary.FConditionFilter22(c,code1,code2,sub)
	local code=c:GetCode()
	return code==code1 or code==code2 or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCode2(code1,code2,sub,insf)
	--g:Material group(nil for Instant Fusion)
	--gc:Material already used
	--chkf: check field, default:PLAYER_NONE
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return g:IsExists(Auxiliary.FConditionFilter21,1,nil,code1,code2) end
				local b1=0 local b2=0 local bs=0
				local f1=false local f2=false local fs=false
				local tc=g:GetFirst()
				while tc do
					local code=tc:GetCode()
					if code==code1 then b1=1 if Auxiliary.FConditionCheckF(tc,chkf) then f1=true end
					elseif code==code2 then b2=1 if Auxiliary.FConditionCheckF(tc,chkf) then f2=true end
					elseif tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then bs=1 if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end
					end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					if sub then	return (b1~=0 and f1 and b2+bs~=0) or (b2~=0 and f2 and b1+bs~=0) or (bs~=0 and fs and b1+b2~=0)
					else return (b1~=0 and f1 and b2==1) or (b2~=0 and f2 and b1==1) end
				else
					if sub then return b1+b2+bs>1
					else return b1+b2==2 end
				end
			end
end
function Auxiliary.FOperationCode2(code1,code2,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=g:FilterSelect(tp,Auxiliary.FConditionFilter21,1,1,nil,code1,code2)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilter22,nil,code1,code2,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				if g1:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCode3(c,code1,code2,code3,sub,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCode3(code1,code2,code3,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCode3(code1,code2,code3,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilter31(c,code1,code2,code3)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3
end
function Auxiliary.FConditionFilter32(c,code1,code2,code3,sub)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3 or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCode3(code1,code2,code3,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then
					local b1=0 local b2=0 local b3=0
					local tc=g:GetFirst()
					while tc do
						local code=tc:GetCode()
						if code==code1 then b1=1
						elseif code==code2 then b2=1
						elseif code==code3 then b3=1
						end
						tc=g:GetNext()
					end
					return b1+b2+b3>1
				end
				local b1=0 local b2=0 local b3=0 local bs=0
				local f1=false local f2=false local f3=false local fs=false
				local tc=g:GetFirst()
				while tc do
					local code=tc:GetCode()
					if code==code1 then b1=1 if Auxiliary.FConditionCheckF(tc,chkf) then f1=true end
					elseif code==code2 then b2=1 if Auxiliary.FConditionCheckF(tc,chkf) then f2=true end
					elseif code==code3 then b3=1 if Auxiliary.FConditionCheckF(tc,chkf) then f3=true end
					elseif tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then bs=1 if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end
					end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					if sub then	return (b1~=0 and f1 and b2+b3+bs>1) or (b2~=0 and f2 and b1+b3+bs>1)
						or (b3~=0 and f3 and b1+b2+bs>1) or (bs~=0 and fs and b1+b2+b3>1)
					else return (b1~=0 and f1 and b2+b3==2) or (b2~=0 and f2 and b1+b3==2)
						or (b3~=0 and f3 and b1+b2==2) end
				else
					if sub then return b1+b2+b3+bs>2
					else return b1+b2+b3==3 end
				end
			end
end
function Auxiliary.FOperationCode3(code1,code2,code3,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					local sg=eg:Filter(Auxiliary.FConditionFilter31,nil,code1,code2,code3)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,1,1,nil)
					sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,1,1,nil)
					g1:Merge(g2)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilter32,nil,code1,code2,code3,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				if g1:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				if g2:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g2:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g3=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				g1:Merge(g3)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCode4(c,code1,code2,code3,code4,sub,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCode4(code1,code2,code3,code4,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCode4(code1,code2,code3,code4,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilter41(c,code1,code2,code3,code4)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3 or code==code4
end
function Auxiliary.FConditionFilter42(c,code1,code2,code3,code4,sub)
	local code=c:GetCode()
	return code==code1 or code==code2 or code==code3 or code==code4 or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCode4(code1,code2,code3,code4,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then
					local b1=0 local b2=0 local b3=0 local b4=0
					local tc=g:GetFirst()
					while tc do
						local code=tc:GetCode()
						if code==code1 then b1=1
						elseif code==code2 then b2=1
						elseif code==code3 then b3=1
						elseif code==code4 then b4=1
						end
						tc=g:GetNext()
					end
					return b1+b2+b3+b4>2
				end
				local b1=0 local b2=0 local b3=0 local b4=0 local bs=0
				local f1=false local f2=false local f3=false local f4=false local fs=false
				local tc=g:GetFirst()
				while tc do
					local code=tc:GetCode()
					if code==code1 then b1=1 if Auxiliary.FConditionCheckF(tc,chkf) then f1=true end
					elseif code==code2 then b2=1 if Auxiliary.FConditionCheckF(tc,chkf) then f2=true end
					elseif code==code3 then b3=1 if Auxiliary.FConditionCheckF(tc,chkf) then f3=true end
					elseif code==code4 then b4=1 if Auxiliary.FConditionCheckF(tc,chkf) then f4=true end
					elseif tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then bs=1 if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end
					end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					if sub then	return (b1~=0 and f1 and b2+b3+b4+bs>2) or (b2~=0 and f2 and b1+b3+b4+bs>2)
						or (b3~=0 and f3 and b1+b2+b4+bs>2) or (b4~=0 and f4 and b1+b2+b3+bs>2)
						or (bs~=0 and fs and b1+b2+b3+b4>2)
					else return (b1~=0 and f1 and b2+b3+b4==3) or (b2~=0 and f2 and b1+b3+b4==3)
						or (b3~=0 and f3 and b1+b2+b4==3) or (b4~=0 and f4 and b1+b2+b3==3) end
				else
					if sub then return b1+b2+b3+b4+bs>3
					else return b1+b2+b3+b4==4 end
				end
			end
end
function Auxiliary.FOperationCode4(code1,code2,code3,code4,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					local sg=eg:Filter(Auxiliary.FConditionFilter41,nil,code1,code2,code3,code4)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,1,1,nil)
					sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,1,1,nil)
					sg:Remove(Card.IsCode,nil,g2:GetFirst():GetCode())
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g3=sg:Select(tp,1,1,nil)
					g1:Merge(g2)
					g1:Merge(g3)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilter42,nil,code1,code2,code3,code4,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				if g1:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g1:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				if g2:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g2:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g3=sg:Select(tp,1,1,nil)
				if g3:GetFirst():IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
					sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				else sg:Remove(Card.IsCode,nil,g3:GetFirst():GetCode()) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g4=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				g1:Merge(g3)
				g1:Merge(g4)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCodeFun(c,code,f,cc,sub,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCodeFun(code,f,cc,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCodeFun(code,f,cc,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilterCF(c,g2,cc)
	return g2:IsExists(aux.TRUE,cc,c)
end
function Auxiliary.FConditionCodeFun(code,f,cc,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return (gc:IsCode(code) or (sub and gc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)))
					and g:IsExists(f,cc,nil) end
				local g1=Group.CreateGroup() local g2=Group.CreateGroup() local fs=false
				local tc=g:GetFirst()
				while tc do
					if tc:IsCode(code) or (sub and tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
						then g1:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					if f(tc) then g2:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					return fs and g1:IsExists(Auxiliary.FConditionFilterCF,1,nil,g2,cc)
				else return g1:IsExists(Auxiliary.FConditionFilterCF,1,nil,g2,cc) end
			end
end
function Auxiliary.FOperationCodeFun(code,f,cc,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=eg:FilterSelect(tp,f,cc,cc,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg1=Group.CreateGroup() local sg2=Group.CreateGroup() local fs=false
				local tc=eg:GetFirst()
				while tc do
					if tc:IsCode(code) or (sub and tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)) then sg1:AddCard(tc) end
					if f(tc) then sg2:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					tc=eg:GetNext()
				end
				if chkf~=PLAYER_NONE then
					local g1=nil
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					if fs then g1=sg1:Select(tp,1,1,nil)
					else g1=sg1:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf) end
					local tc1=g1:GetFirst()
					sg2:RemoveCard(tc1)
					if Auxiliary.FConditionCheckF(tc1,chkf) or sg2:GetCount()==cc then
						Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
						local g2=sg2:Select(tp,cc,cc,tc1)
						g1:Merge(g2)
					else
						Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
						local g2=sg2:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,tc1,chkf)
						g1:Merge(g2)
						if cc>1 then
							sg2:RemoveCard(g2:GetFirst())
							Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
							g2=sg2:Select(tp,cc-1,cc-1,tc1)
							g1:Merge(g2)
						end
					end
					Duel.SetFusionMaterial(g1)
				else
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg1:Select(tp,1,1,nil)
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg2:Select(tp,cc,cc,g1:GetFirst())
					g1:Merge(g2)
					Duel.SetFusionMaterial(g1)
				end
			end
end
function Auxiliary.AddFusionProcFun2(c,f1,f2,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionFun2(f1,f2,insf))
	e1:SetOperation(Auxiliary.FOperationFun2(f1,f2,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilterF2(c,g2)
	return g2:IsExists(aux.TRUE,1,c)
end
function Auxiliary.FConditionFilterF2c(c,f1,f2)
	return f1(c) or f2(c)
end
function Auxiliary.FConditionFilterF2r(c,f1,f2)
	return f1(c) and not f2(c)
end
function Auxiliary.FConditionFun2(f1,f2,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return (f1(gc) and g:IsExists(f2,1,nil))
					or (f2(gc) and g:IsExists(f1,1,nil)) end
				local g1=Group.CreateGroup() local g2=Group.CreateGroup() local fs=false
				local tc=g:GetFirst()
				while tc do
					if f1(tc) then g1:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					if f2(tc) then g2:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
					tc=g:GetNext()
				end
				if chkf~=PLAYER_NONE then
					return fs and g1:IsExists(Auxiliary.FConditionFilterF2,1,nil,g2)
				else return g1:IsExists(Auxiliary.FConditionFilterF2,1,nil,g2) end
			end
end
function Auxiliary.FOperationFun2(f1,f2,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					local sg=eg:Filter(Auxiliary.FConditionFilterF2c,nil,f1,f2)
					local b1=f1(gc)
					local b2=f2(gc)
					if b1 and not b2 then sg:Remove(f1,nil) end
					if b2 and not b1 then sg:Remove(f2,nil) end
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,1,1,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilterF2c,nil,f1,f2)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then
					g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				local tc1=g1:GetFirst()
				sg:RemoveCard(tc1)
				local b1=f1(tc1)
				local b2=f2(tc1)
				if b1 and not b2 then sg:Remove(Auxiliary.FConditionFilterF2r,nil,f1,f2) end
				if b2 and not b1 then sg:Remove(Auxiliary.FConditionFilterF2r,nil,f2,f1) end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g2=sg:Select(tp,1,1,nil)
				g1:Merge(g2)
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcCodeRep(c,code,cc,sub,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionCodeRep(code,cc,sub,insf))
	e1:SetOperation(Auxiliary.FOperationCodeRep(code,cc,sub,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFilterCR(c,code,sub)
	return c:IsCode(code) or (sub and c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE))
end
function Auxiliary.FConditionCodeRep(code,cc,sub,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return gc:IsHasEffect() and g:IsExists(Card.IsCode,cc-1,nil,code) end
				local g1=g:Filter(Card.IsCode,nil,code)
				if not sub then
					if chkf~=PLAYER_NONE then return g1:GetCount()>=cc and g1:FilterCount(Card.IsOnField,nil)~=0
					else return g1:GetCount()>=cc end
				end
				local g2=g:Filter(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
				if chkf~=PLAYER_NONE then
					return (g1:FilterCount(Card.IsOnField,nil)~=0 or g2:FilterCount(Card.IsOnField,nil)~=0)
						and g1:GetCount()>=cc-1 and g1:GetCount()+g2:GetCount()>=cc
				else return g1:GetCount()>=cc-1 and g1:GetCount()+g2:GetCount()>=cc end
			end
end
function Auxiliary.FOperationCodeRep(code,cc,sub,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=eg:FilterSelect(tp,Card.IsCode,cc-1,cc-1,nil,code)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(Auxiliary.FConditionFilterCR,nil,code,sub)
				local g1=nil
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				if chkf~=PLAYER_NONE then g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				else g1=sg:Select(tp,1,1,nil) end
				local tc1=g1:GetFirst()
				for i=1,cc-1 do
					if tc1:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then sg:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
					else sg:RemoveCard(tc1) end
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,1,1,nil)
					tc1=g2:GetFirst()
					g1:Merge(g2)
				end
				Duel.SetFusionMaterial(g1)
			end
end
function Auxiliary.AddFusionProcFunRep(c,f,cc,insf)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(Auxiliary.FConditionFunRep(f,cc,insf))
	e1:SetOperation(Auxiliary.FOperationFunRep(f,cc,insf))
	c:RegisterEffect(e1)
end
function Auxiliary.FConditionFunRep(f,cc,insf)
	return	function(e,g,gc,chkf)
				if g==nil then return insf end
				if gc then return f(gc) and g:IsExists(f,cc-1,nil) end
				local g1=g:Filter(f,nil)
				if chkf~=PLAYER_NONE then
					return g1:FilterCount(Card.IsOnField,nil)~=0 and g1:GetCount()>=cc
				else return g1:GetCount()>=cc end
			end
end
function Auxiliary.FOperationFunRep(f,cc,insf)
	return	function(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
				if gc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=eg:FilterSelect(tp,f,cc-1,cc-1,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				local sg=eg:Filter(f,nil)
				if chkf==PLAYER_NONE or sg:GetCount()>=cc then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g1=sg:Select(tp,cc,cc,nil)
					Duel.SetFusionMaterial(g1)
					return
				end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				local g1=sg:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf)
				local tc1=g1:GetFirst()
				sg:RemoveCard(tc1)
				if cc>1 then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g2=sg:Select(tp,cc-1,cc-1,nil)
					g1:Merge(g2)
				end
				Duel.SetFusionMaterial(g1)
			end
end
